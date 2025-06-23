// server.js (Token-only version)
require("dotenv").config();
const express = require("express");
const axios = require("axios");
const fs = require("fs");
const path = require("path");

const app = express();
const port = process.env.PORT || 3000;
const CONFIG_PATH = path.join(__dirname, "config.json");

const startKwhMap = {}; // 缓存每日电量起点值

// 读取配置（token 和 sn）
function loadUserConfig() {
  if (fs.existsSync(CONFIG_PATH)) {
    return JSON.parse(fs.readFileSync(CONFIG_PATH));
  }
  return {
    token: process.env.IAMMETER_TOKEN,
    sn: process.env.DEFAULT_SN
  };
}

// 获取初始电量（按天）
async function getStartKwh(sn, date, token) {
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}&rtctime=${date}`;
  const res = await axios.get(url);
  const values = res.data?.data?.values;
  if (!values || !Array.isArray(values)) {
    throw new Error("⚠️ 无法获取初始电量数据");
  }
  return { snapshot: values };
}

// 获取当前电量（分相）
async function getCurrentKwh(sn, token) {
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}`;
  console.log("📡 请求 URL:", url);
  const res = await axios.get(url);
console.log("📡 响应数据：", JSON.stringify(res.data, null, 2));

  const values = res.data?.data?.values;
  if (!values || !Array.isArray(values)) {
    throw new Error("⚠️ 无法获取当前电表数据");
  }

  const phases = {};
  values.forEach((v, idx) => {
    const name = ["A", "B", "C"][idx] || `P${idx + 1}`;
    phases[name] = {
      importKwh: v[3],
      exportKwh: v[4]
    };
  });

  return {
    snapshot: values,
    phases
  };
}

// 实时数据 API
app.get("/api/data/:sn", async (req, res) => {
  const sn = req.params.sn;
  const token = req.query.token || loadUserConfig().token;  // 优先使用前端传入

  try {
    const current = await getCurrentKwh(sn, token);
    res.json(current);
  } catch (err) {
    console.error("❌ 获取电表数据失败：", err.message);
    res.status(500).json({ error: "数据获取失败", detail: err.message });
  }
});
// 每日电量 API
app.get("/api/daily-energy/:sn", async (req, res) => {
  const sn = req.params.sn;
  const token = req.query.token || loadUserConfig().token;
  const date = req.query.date;

  try {
    if (!startKwhMap[sn] || (date && startKwhMap[sn].date !== date)) {
      const init = await getStartKwh(sn, date, token);
      startKwhMap[sn] = { date, phases: {} };
      init.snapshot.forEach((v, idx) => {
        const name = ["A", "B", "C"][idx] || `P${idx + 1}`;
        startKwhMap[sn].phases[name] = {
          importKwh: v[3],
          exportKwh: v[4]
        };
      });
    }

    const current = await getCurrentKwh(sn, token);
    const start = startKwhMap[sn];
    const result = {};
    for (const phase in current.phases) {
      const usedImport = current.phases[phase].importKwh - start.phases[phase].importKwh;
      const usedExport = current.phases[phase].exportKwh - start.phases[phase].exportKwh;

      result[phase] = {
        importUsedKwh: parseFloat(usedImport.toFixed(3)),
        exportUsedKwh: parseFloat(usedExport.toFixed(3))
      };
    }

    res.json({
      date: start.date,
      phases: result,
      snapshot: current.snapshot
    });
  } catch (err) {
    console.error("❌ 获取每日电量失败:", err.message);
    res.status(500).json({ error: err.message });
  }
});
//api rank data
app.get("/api/compare/rank/:date", (req, res) => {
  const date = req.params.date;
  const filePath = path.join(DATA_DIR, `${date}.txt`);

  if (!fs.existsSync(filePath)) {
    return res.json([]);
  }

  const lines = fs.readFileSync(filePath, "utf-8").trim().split("\n");
  const records = lines
    .map(line => {
      try {
        return JSON.parse(line);
      } catch {
        return null;
      }
    })
    .filter(Boolean);

  res.json(records);
});


//save setting of sn and token
app.use(express.json());

app.post("/api/save-config", (req, res) => {
    console.log("📥 POST /api/save-config reached"); 
  const { sn, token } = req.body;
  if (!sn || !token) {
    return res.status(400).send("Missing SN or token");
  }

  const newConfig = { sn, token };
  fs.writeFileSync(CONFIG_PATH, JSON.stringify(newConfig, null, 2));
  console.log("✅ Config saved from frontend:", newConfig);
  res.send("OK");
});
// api compare
const DATA_DIR = path.join(__dirname, "compare-data");
if (!fs.existsSync(DATA_DIR)) fs.mkdirSync(DATA_DIR);

app.post("/api/compare", (req, res) => {
  const { sn, date, generation_today, export_today, consumption_today, self_use_rate } = req.body;

  if (!sn || !date) {
    return res.status(400).json({ error: "Missing sn or date" });
  }

  const filePath = path.join(DATA_DIR, `${date}.txt`);
  const newEntry = {
    sn,
    generation_today: parseFloat(generation_today),
    export_today: parseFloat(export_today),
    consumption_today: parseFloat(consumption_today),
    self_use_rate: parseFloat(self_use_rate),
    timestamp: new Date().toISOString()
  };

  // 读取旧数据并去重（保留其他sn，替换当前sn）
  let records = [];
  if (fs.existsSync(filePath)) {
    const lines = fs.readFileSync(filePath, "utf-8").trim().split("\n");
    records = lines
      .map(line => {
        try {
          return JSON.parse(line);
        } catch {
          return null;
        }
      })
      .filter(Boolean)
      .filter(r => r.sn !== sn); // 移除旧的该sn记录
  }

  records.push(newEntry); // 添加当前记录
  fs.writeFileSync(filePath, records.map(r => JSON.stringify(r)).join("\n")); // 重写文件

  function getRank(field) {
    const sorted = [...records].sort((a, b) => b[field] - a[field]);
    return sorted.findIndex(r => r.sn === sn) + 1;
  }

  res.json({
    user_position: records.length,
    generation_rank: getRank("generation_today"),
    consumption_rank: getRank("consumption_today"),
    export_rank: getRank("export_today"),
    self_use_rate_rank: getRank("self_use_rate")
  });
});



// 提供静态页面服务
app.use(express.static(path.join(__dirname, "public")));

// 启动服务器
app.listen(port, () => {
  console.log(`🚀 Server running at http://localhost:${port}`);
});
