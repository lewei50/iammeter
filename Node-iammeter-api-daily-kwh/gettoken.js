const axios = require("axios");

const email = "laoliu";   // 替换成你的 IAMMETER 登录账号
const password = "lewei502308";         // 替换成你的密码

async function getToken() {
  // 准备请求体
  const postData = {
    userName: email,
    password: password
  };

  console.log("📤 即将提交的数据:");
  console.log(JSON.stringify(postData, null, 2));  // 打印 POST 的 JSON 数据

  try {
    // 发送 POST 请求
    const response = await axios.post("https://www.iammeter.com/api/v1/user/login", postData);

    // 打印服务器返回的完整响应对象
    console.log("📥 服务器返回的数据:");
    console.log(JSON.stringify(response.data, null, 2));  // 美化打印返回的 JSON

    // 如果成功获取 token
    if (response.data.data.token) {
      console.log("✅ 登录成功！Token 是：", response.data.data.token);
    } else {
      console.error("⚠️ 登录返回中没有找到 token。");
    }
  } catch (error) {
    console.error("🚨 请求出错：", error.message);

    // 如果服务器返回了错误响应
    if (error.response) {
      console.error("📥 错误响应体:");
      console.log(JSON.stringify(error.response.data, null, 2));
    }
  }
}

getToken();
