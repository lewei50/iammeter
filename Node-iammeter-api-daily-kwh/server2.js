require('dotenv').config();
const express = require("express");
const axios = require("axios");
const fs = require("fs");
const path = require("path");

const app = express();
const port = process.env.PORT || 3000;

let token = null;
const startKwhMap = {}; // 缓存：{ sn: { date, importKwh, exportKwh } }

// 允许读取 JSON 请求体
app.use(express.json());

// 配置文件路径
const CONFIG_PATH = path.join(__dirname, "config.json");

// 从文件或 .env 中读取配置
function loadUserConfig() {
  if (fs.existsSync(CONFIG_PATH)) {
    return JSON.parse(fs.readFileSync(CONFIG_PATH));
  }
  return {
    email: process.env.IAMMETER_EMAIL,
    password: process.env.IAMMETER_PASSWORD,
    sn: process.env.DEFAULT_SN
  };
}

// 保存配置到本地文件
function saveUserConfig(config) {
  fs.writeFileSync(CONFIG_PATH, JSON.stringify(config, null, 2));
}

// 获取 token（自动缓存）
async function getToken(email, password) {
  console.log("📡 请求 token，用账号:", email,password);
  if (token) return;
  const res = await axios.post("https://www.iammeter.com/api/v1/user/login", {
    userName: email,
    password: password,
  });

  console.log("🟢 登录接口返回：", JSON.stringify(res.data, null, 2));
  token = res.data.data.token;
  console.log(`✅ Token 已获取：${token}`);
}

// 获取某日最后一组数据（用于起始 import/export kWh）
async function getStartKwh(sn, date, email, password) {
  await getToken(email, password);
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}&rtctime=${date}`;
  const res = await axios.get(url);
  const values = res.data?.data?.values;

  if (!values || !Array.isArray(values) || values.length === 0) {
    throw new Error("⚠️ 无法获取初始电量数据");
  }

  return {
    snapshot: values  // 数组：每相数据，单相为一组，三相为三组
  };
}
// 获取当前最新数据（无 rtctime）
async function getCurrentKwh(sn, email, password) {
  await getToken(email, password);
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}`;
  const res = await axios.get(url);
  const values = res.data?.data?.values;

  if (!values || !Array.isArray(values) || values.length === 0) {
    throw new Error("⚠️ 无法获取当前电表数据");
  }

  const phases = {};
  values.forEach((v, idx) => {
    const phaseName = ["A", "B", "C"][idx] || `P${idx + 1}`;
    phases[phaseName] = {
      importKwh: v[3],
      exportKwh: v[4]
    };
  });

  return {
    snapshot: values,
    phases
  };
}
// 接口：保存设置
app.post("/api/save-settings", (req, res) => {
  const { email, password, sn } = req.body;
  if (!email || !password || !sn) {
    return res.status(400).json({ error: "缺少必要字段" });
  }
  saveUserConfig({ email, password, sn });
  token = null; // 重置 token 缓存
  console.log(`⚙️ 保存新配置：email=${email}, sn=${sn}`);
  res.json({ message: "设置已保存" });
});

// 接口：获取当前设置
app.get("/api/settings", (req, res) => {
  const config = loadUserConfig();
  res.json(config);
});

// 🆕 获取当日电量增量（首次需提供 ?date 参数）
app.get("/api/daily-energy/:sn", async (req, res) => {
  const { email, password } = loadUserConfig();
  const sn = req.params.sn;
  const date = req.query.date;

  try {
    // 初始化或换日时更新缓存
    if (!startKwhMap[sn] || (date && startKwhMap[sn].date !== date)) {
      const init = await getStartKwh(sn, date, email, password);

      startKwhMap[sn] = {
        date,
        phases: {}
      };

      init.snapshot.forEach((v, idx) => {
        const phaseName = ["A", "B", "C"][idx] || `P${idx + 1}`;
        startKwhMap[sn].phases[phaseName] = {
          importKwh: v[3],
          exportKwh: v[4]
        };
      });

      console.log(`📌 已缓存初始 kWh：${sn} @ ${date}`);
    }

    // 获取当前数据（含 phases）
    const current = await getCurrentKwh(sn, email, password);
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

// get sn（默认 SN）
app.get("/api/default-sn", (req, res) => {
  const config = loadUserConfig();
  res.json({ sn: config.sn });
});

// 实时数据接口
app.get("/api/data/:sn", async (req, res) => {
  const sn = req.params.sn;
  const { email, password } = loadUserConfig();
  await getToken(email, password);
  try {
    const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}`;
    const response = await axios.get(url);
    const values = response.data.data.values;
    const result = {
      success: true,
      gmtTime: response.data.data.gmtTime,
      localTime: response.data.data.localTime,
      phases: []
    };

    values.forEach((v, index) => {
      result.phases.push({
        phase: ['A', 'B', 'C'][index] || `P${index + 1}`,
        voltage: v[0],
        current: v[1],
        power: v[2],
        importKwh: v[3],
        exportKwh: v[4]
      });
    });

    res.json(result);

  } catch (err) {
    if (err.response && err.response.status === 401) {
      token = null;
    }
    res.status(500).send({ error: "获取电表数据失败", detail: err.message });
  }
});

// 提供前端静态页面（放在 public/ 目录下）
app.use(express.static(path.join(__dirname, "public")));

// 启动服务器
app.listen(port, () => {
  console.log(`🚀 Server running at http://localhost:${port}`);
});
