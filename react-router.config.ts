import type { Config } from "@react-router/dev/config";
import { readdirSync, readFileSync } from "node:fs";
import { resolve } from "node:path";
import { parse } from "yaml";

const basePath = (
  process.env.ROUTER_BASE_PATH ??
  process.env.VITE_BASE_PATH ??
  ""
).replace(/\/$/, "");
const snippetDirectory = resolve("snippets");
const snippetPaths = readdirSync(snippetDirectory)
  .filter(
    (filename) => filename.endsWith(".hh") && filename !== "test-support.hh",
  )
  .map((filename) => {
    const source = readFileSync(resolve(snippetDirectory, filename), "utf8");
    const frontmatter = source.match(
      /^\/\* cpsc:meta:start\r?\n([\s\S]*?)\r?\ncpsc:meta:end \*\//,
    );
    if (!frontmatter) throw new Error(`Snippet metadata is missing: ${filename}`);
    const metadata = parse(frontmatter[1]) as { slug?: string };
    if (!metadata.slug) throw new Error(`Snippet slug is missing: ${filename}`);
    return `/snippets/${metadata.slug}`;
  });

export default {
  ssr: false,
  basename: basePath || "/",
  prerender: ["/", ...snippetPaths],
  routeDiscovery: { mode: "initial" },
} satisfies Config;
