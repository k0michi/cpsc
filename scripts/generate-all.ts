import { readFileSync, readdirSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";

const snippetsDirectory = resolve("snippets");
const outputPath = resolve(snippetsDirectory, "all.hh");
const check = process.argv.includes("--check");

const snippetFiles = readdirSync(snippetsDirectory)
  .filter(
    (filename) =>
      filename.endsWith(".hh") &&
      filename !== "all.hh" &&
      filename !== "test-support.hh",
  )
  .sort();

const generated = [
  "#pragma once",
  "",
  ...snippetFiles.map((filename) => `#include "${filename}"`),
  "",
].join("\n");

if (check) {
  const existing = readFileSync(outputPath, "utf8");
  if (existing !== generated) {
    throw new Error(
      "snippets/all.hh is out of date. Run: npm run generate:all",
    );
  }
} else {
  writeFileSync(outputPath, generated);
}
