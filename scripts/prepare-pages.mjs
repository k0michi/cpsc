import {
  cpSync,
  existsSync,
  mkdirSync,
  readdirSync,
  readFileSync,
  rmSync,
  statSync,
  writeFileSync,
} from "node:fs";
import { join, resolve } from "node:path";

const clientDirectory = resolve("build/client");
const pagesDirectory = resolve("build/pages");
const basePath = (process.env.PAGES_BASE_PATH ?? "").replace(/\/$/, "");
const renderedDirectory = basePath
  ? resolve(clientDirectory, `.${basePath}`)
  : clientDirectory;

if (!existsSync(renderedDirectory)) {
  throw new Error(`Prerendered site was not found: ${renderedDirectory}`);
}

rmSync(pagesDirectory, { recursive: true, force: true });
mkdirSync(pagesDirectory, { recursive: true });

for (const entry of readdirSync(renderedDirectory)) {
  if (entry === ".vite" || entry === "pages") continue;
  cpSync(join(renderedDirectory, entry), join(pagesDirectory, entry), {
    recursive: true,
  });
}

for (const sharedEntry of ["assets"]) {
  const source = join(clientDirectory, sharedEntry);
  if (existsSync(source)) {
    cpSync(source, join(pagesDirectory, sharedEntry), { recursive: true });
  }
}

const textExtensions = new Set([".html", ".js", ".css", ".data"]);

function rewriteAssetPaths(directory) {
  for (const entry of readdirSync(directory)) {
    const path = join(directory, entry);
    if (statSync(path).isDirectory()) {
      rewriteAssetPaths(path);
      continue;
    }
    const extension = entry.slice(entry.lastIndexOf("."));
    if (!basePath || !textExtensions.has(extension)) continue;

    const source = readFileSync(path, "utf8");
    const rewritten = source.replaceAll("/assets/", `${basePath}/assets/`);
    writeFileSync(path, rewritten);
  }
}

rewriteAssetPaths(pagesDirectory);

if (!existsSync(join(pagesDirectory, "index.html"))) {
  throw new Error("GitHub Pages artifact does not contain index.html");
}
