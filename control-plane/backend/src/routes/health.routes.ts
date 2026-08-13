import { Router } from "express";
import { isAlive } from "../controllers/health.controller.js";

export const healthRouter = Router();

healthRouter.get("/isAlive", isAlive);
