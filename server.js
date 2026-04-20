const express = require("express");
const cors = require("cors");
const { SerialPort } = require("serialport");
const { ReadlineParser } = require("@serialport/parser-readline");

const app = express();
app.use(cors());

let cars = 0;

// 🔴 ndrysho COM port
const port = new SerialPort({
  path: "COM3",
  baudRate: 9600
});

const parser = port.pipe(new ReadlineParser({ delimiter: "\n" }));

parser.on("data", (line) => {
  console.log("Arduino:", line);

  if (line.includes("CARS:")) {
    cars = parseInt(line.replace(/\D/g, ""));
  }
});

app.get("/data", (req, res) => {
  res.json({ cars });
});

app.listen(3000, () => {
  console.log("Server: http://localhost:3000");
});
