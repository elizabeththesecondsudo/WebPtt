import cors from "cors";
import express from "express";
import { errorHandler, notFoundHandler } from "./middleware/error.middleware.js";
import { healthRouter } from "./routes/health.routes.js";

export const app = express();

app.use(cors());
app.use(express.json());

app.use(healthRouter);

app.use(notFoundHandler);
app.use(errorHandler);
