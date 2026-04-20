const express = require("express");
const cors = require("cors");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");

const app = express();
app.use(cors());

let cars = 0;

// 🔌 CHANGE THIS PORT
const port = new SerialPort({
  path: "COM8",
  baudRate: 9600
});

const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

parser.on("data", (data) => {
  data = data.trim();
  console.log("Arduino:", data);

  // 👇 EXACT si kodi yt
  if (data.startsWith("CARS:")) {
    cars = parseInt(data.replace("CARS:", ""));
  }
});

app.get("/data", (req, res) => {
  res.json({
    s1: cars >= 1 ? 1 : 0,
    s2: cars >= 2 ? 1 : 0,
    s3: cars >= 3 ? 1 : 0
  });
});

app.listen(3000, () => {
  console.log("Server running on http://localhost:3000");
});
