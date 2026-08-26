import { parse } from "yaml";

export type Snippet = {
  slug: string;
  title: string;
  shortTitle: string;
  category: string;
  description?: string | null;
  text?: string | null;
  language: {
    name: string;
    version: string;
    prism: string;
  };
  validation?: Validation[] | null;
  code: string;
  subsnippets: Subsnippet[];
  blocks: SnippetBlock[];
  sourceFile: string;
  tests: TestCase[];
};

export type Validation = {
  label: string;
  url: string;
};

export type Subsnippet = {
  title: string;
  description?: string | null;
  code: string;
};

export type SnippetBlock =
  | { type: "text"; source: string }
  | { type: "code"; subsnippetIndex: number };

export type TestCase = {
  title: string;
  code: string;
  sourceFile: string;
};

const snippetFiles = import.meta.glob<string>(
  ["../../snippets/*.hh", "!../../snippets/test-support.hh"],
  {
    query: "?raw",
    import: "default",
    eager: true,
  },
);
const testFiles = import.meta.glob<string>("../../snippets/*.test.cc", {
  query: "?raw",
  import: "default",
  eager: true,
});

const frontmatterPattern =
  /^\/\* cpsc:meta:start\r?\n([\s\S]*?)\r?\ncpsc:meta:end \*\/\r?\n?/;

function parseSnippet(path: string, source: string): Omit<Snippet, "tests"> {
  const match = source.match(frontmatterPattern);
  if (!match) {
    throw new Error(`Snippet metadata is missing: ${path}`);
  }

  const metadata = parse(match[1]) as Omit<
    Snippet,
    "code" | "subsnippets" | "blocks" | "sourceFile" | "tests"
  >;
  const required = [
    metadata.slug,
    metadata.title,
    metadata.shortTitle,
    metadata.category,
    metadata.language?.name,
    metadata.language?.version,
    metadata.language?.prism,
  ];

  if (required.some((value) => value === undefined || value === "")) {
    throw new Error(`Snippet metadata is incomplete: ${path}`);
  }
  if (
    metadata.validation?.some(
      (validation) => !validation.label || !validation.url,
    )
  ) {
    throw new Error(`Snippet validation metadata is incomplete: ${path}`);
  }
  const rawCode = source.slice(match[0].length).trimStart();
  const textBlockPattern =
    /\/\* cpsc:text:start\r?\n([\s\S]*?)\s*cpsc:text:end \*\//gm;
  const subsnippetPattern =
    /^\/\/ cpsc:subsnippet:start (.+)\r?\n(?:\/\/ cpsc:subsnippet:description (.+)\r?\n)?([\s\S]*?)^\/\/ cpsc:subsnippet:end\s*$/gm;
  const subsnippetMatches = Array.from(rawCode.matchAll(subsnippetPattern));
  const subsnippets = subsnippetMatches.map((subsnippet) => ({
      title: subsnippet[1].trim(),
      description: subsnippet[2]?.trim() || null,
      code: subsnippet[3].trim(),
    }));
  if (subsnippets.length === 0) {
    throw new Error(`At least one subsnippet is required: ${path}`);
  }
  const textMatches = Array.from(rawCode.matchAll(textBlockPattern));
  const blocks: SnippetBlock[] = [
    ...textMatches.map((text) => ({
      type: "text" as const,
      source: text[1].trim(),
      position: text.index,
    })),
    ...subsnippetMatches.map((subsnippet, subsnippetIndex) => ({
      type: "code" as const,
      subsnippetIndex,
      position: subsnippet.index,
    })),
  ]
    .sort((a, b) => a.position - b.position)
    .map(({ position: _, ...block }) => block);

  return {
    ...metadata,
    text: textMatches.map((text) => text[1].trim()).join("\n\n") || null,
    code: subsnippets.map((subsnippet) => subsnippet.code).join("\n\n"),
    subsnippets,
    blocks,
    sourceFile: path.split("/").at(-1) ?? path,
  };
}

const parsedSnippets = Object.entries(snippetFiles)
  .map(([path, source]) => parseSnippet(path, source))
  .sort(
    (a, b) =>
      a.category.localeCompare(b.category, "en") ||
      a.title.localeCompare(b.title, "en"),
  );

const duplicateSlug = parsedSnippets.find(
  (snippet, index) =>
    parsedSnippets.findIndex((candidate) => candidate.slug === snippet.slug) !== index,
);
if (duplicateSlug) {
  throw new Error(`Duplicate snippet slug: ${duplicateSlug.slug}`);
}

const testsBySnippet = new Map<string, TestCase[]>();
const testCasePattern =
  /^\/\/ cpsc:test:start\s*\r?\n([\s\S]*?)^\/\/ cpsc:test:end\s*$/gm;

for (const [path, source] of Object.entries(testFiles)) {
  const frontmatter = source.match(frontmatterPattern);
  if (!frontmatter) throw new Error(`Test metadata is missing: ${path}`);

  const metadata = parse(frontmatter[1]) as { snippet?: string };
  if (!metadata.snippet) throw new Error(`Test snippet is missing: ${path}`);

  const cases = Array.from(source.matchAll(testCasePattern), (match) => {
    const code = match[1].trim();
    const testName = code.match(
      /\bTEST\s*\(\s*([A-Za-z_]\w*)\s*,\s*([A-Za-z_]\w*)\s*\)/,
    );
    if (!testName) throw new Error(`GoogleTest name is missing: ${path}`);
    return {
      title: `${testName[1]}.${testName[2]}`,
      code,
      sourceFile: path.split("/").at(-1) ?? path,
    };
  });
  if (cases.length === 0) throw new Error(`No marked test cases: ${path}`);

  testsBySnippet.set(metadata.snippet, [
    ...(testsBySnippet.get(metadata.snippet) ?? []),
    ...cases,
  ]);
}

export const snippets: Snippet[] = parsedSnippets.map((snippet) => {
  const tests = testsBySnippet.get(snippet.slug);
  if (!tests) throw new Error(`Tests are missing for snippet: ${snippet.slug}`);
  return { ...snippet, tests };
});

export const categories = Array.from(
  new Set(snippets.map((snippet) => snippet.category)),
).sort((a, b) => a.localeCompare(b, "en"));

export function findSnippet(slug: string) {
  return snippets.find((snippet) => snippet.slug === slug);
}
