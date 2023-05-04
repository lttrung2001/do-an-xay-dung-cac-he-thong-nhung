import express from "express";
import http from "http";
import socketio from "socket.io";

const app = express();
const server = http.createServer(app);
server.listen(process.env.PORT || 3000, () => {
    console.log(`Server is listening on port: ${process.env.PORT || 3000}`);
});
app.get("/", (req, res) => {
    res.send("<h1>Hello world</h1>");
});

const io = socketio(server);
io.on("connection", (socket) => {
    socket.join("DEFAULT");
    socket.on("status", args => {
        socket.to("DEFAULT").emit("status", args);
    });
    socket.on("control", args => {
        socket.to("DEFAULT").emit("control", args);
    });
});