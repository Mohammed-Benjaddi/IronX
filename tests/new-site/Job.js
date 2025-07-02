const mongoose = require("mongoose");

const jobSchema = new mongoose.Schema(
  {
    title: {
      type: String,
      required: [true, "Job title is required"],
      trim: true,
    },
    description: {
      type: String,
      required: [true, "Job description is required"],
    },
    budget: {
      type: Number,
      required: true,
      min: [0, "Budget must be a positive number"],
    },
    location: {
      type: String,
      default: "Remote",
    },
    category: {
      type: String,
      required: true,
    },
    image: {
      type: String, // URL or base64
      default: "",
    },
    isComplete: {
      type: Boolean,
      default: false,
    },
    createdBy: {
      type: mongoose.Schema.Types.ObjectId,
      ref: "User",
      required: true,
    },
    applicants: [
      {
        user: { type: mongoose.Schema.Types.ObjectId, ref: "User" },
        message: String,
        appliedAt: { type: Date, default: Date.now },
      },
    ],
    rating: {
      stars: {
        type: Number,
        min: 1,
        max: 5,
      },
      comment: String,
    },
  },
  {
    timestamps: true, // adds createdAt and updatedAt
  }
);

module.exports = mongoose.model("Job", jobSchema);

------WebKitFormBoundaryTG6aGUozcVItswMO--