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
  if (token) return;
  const res = await axios.post("https://www.iammeter.com/api/v1/user/login", {
    userName: email,
    password: password,
  });
  token = res.data.data.token;
  console.log(`✅ Token 已获取：${token}`);
}

// 获取某日最后一组数据（用于起始 import/export kWh）
async function getStartKwh(sn, date, email, password) {
  await getToken(email, password);
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}&rtctime=${date}`;
  const res = await axios.get(url);
  const values = res.data.data.values[0];
  console.log(`✅ import kwh ${values[3]}, export kwh ${values[4]}`);

  return {
    importKwh: values[3],
    exportKwh: values[4],
  };
}

// 获取当前最新数据（无 rtctime）
async function getCurrentKwh(sn, email, password) {
  await getToken(email, password);
  const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}`;
  const res = await axios.get(url);
  const values = res.data.data.values[0];
  return {
    importKwh: values[3],
    exportKwh: values[4],
    snapshot: values
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
    if (!startKwhMap[sn] || (date && startKwhMap[sn].date !== date)) {
      const init = await getStartKwh(sn, date, email, password);
      startKwhMap[sn] = {
        date,
        importKwh: init.importKwh,
        exportKwh: init.exportKwh
      };
      console.log(`📌 初始电量缓存：${sn} @ ${date}`);
    }

    const current = await getCurrentKwh(sn, email, password);
    const start = startKwhMap[sn];

    const importUsed = (current.importKwh - start.importKwh).toFixed(3);
    const exportUsed = (current.exportKwh - start.exportKwh).toFixed(3);

    res.json({
      date: start.date,
      startImportKwh: start.importKwh,
      startExportKwh: start.exportKwh,
      currentImportKwh: current.importKwh,
      currentExportKwh: current.exportKwh,
      importUsedKwh: parseFloat(importUsed),
      exportUsedKwh: parseFloat(exportUsed),
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
  const { email, password } = loadUserConfig();
  const sn = req.params.sn;
  await getToken(email, password);
  try {
    const url = `https://www.iammeter.com/api/v1/site/meterdata/${sn}?token=${token}`;
    const response = await axios.get(url);
    res.json(response.data);
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
