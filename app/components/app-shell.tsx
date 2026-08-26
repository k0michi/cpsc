import { useEffect, useMemo, useRef, useState } from "react";
import { Link, NavLink, useLocation } from "react-router";
import { categories, snippets } from "../data/snippets";
import { categoryHueStyle, hueFromCategory } from "../utils/category-color";

export function AppShell({
  children,
  category,
}: {
  children: React.ReactNode;
  category?: string;
}) {
  const [query, setQuery] = useState("");
  const [menuOpen, setMenuOpen] = useState(false);
  const shellRef = useRef<HTMLDivElement>(null);
  const searchRef = useRef<HTMLInputElement>(null);
  const location = useLocation();
  const visibleSnippets = useMemo(() => {
    const term = query.trim().toLowerCase();
    if (!term) return snippets;
    return snippets.filter((snippet) =>
      [snippet.title, snippet.shortTitle, snippet.category, snippet.description]
        .some((value) => value?.toLowerCase().includes(term)),
    );
  }, [query]);

  useEffect(() => {
    function focusSearch(event: KeyboardEvent) {
      if (event.key === "/" && document.activeElement?.tagName !== "INPUT") {
        event.preventDefault();
        searchRef.current?.focus();
      }
    }
    window.addEventListener("keydown", focusSearch);
    return () => window.removeEventListener("keydown", focusSearch);
  }, []);

  useEffect(() => {
    setMenuOpen(false);
  }, [location.pathname]);

  useEffect(() => {
    if (!menuOpen) return;

    function closeMenu(event: KeyboardEvent) {
      if (event.key === "Escape") setMenuOpen(false);
    }
    document.body.classList.add("menu-open");
    window.addEventListener("keydown", closeMenu);
    return () => {
      document.body.classList.remove("menu-open");
      window.removeEventListener("keydown", closeMenu);
    };
  }, [menuOpen]);

  useEffect(() => {
    function positionSidebar() {
      shellRef.current?.style.setProperty(
        "--sidebar-top",
        `${Math.max(0, 52 - window.scrollY)}px`,
      );
    }
    positionSidebar();
    window.addEventListener("scroll", positionSidebar, { passive: true });
    return () => window.removeEventListener("scroll", positionSidebar);
  }, []);

  const themeStyle = category
    ? ({
        "--category-hue": hueFromCategory(category, categories),
      } as React.CSSProperties)
    : undefined;

  return (
    <div
      ref={shellRef}
      className={`app-shell ${category ? "has-category" : ""}`}
      style={themeStyle}
    >
      <header className="topbar">
        <Link to="/" className="brand" aria-label="CPSC home">
          <strong>CPSC</strong>
          <span className="brand-full">Competitive Programming Snippet Collection</span>
        </Link>
        <button
          type="button"
          className="menu-toggle"
          aria-label={menuOpen ? "メニューを閉じる" : "メニューを開く"}
          aria-controls="snippet-navigation"
          aria-expanded={menuOpen}
          onClick={() => setMenuOpen((open) => !open)}
        >
          <span />
          <span />
          <span />
        </button>
        <a
          className="github-link"
          href="https://github.com/k0michi/cpsc"
          target="_blank"
          rel="noreferrer"
          aria-label="k0michi/cpscをGitHubで開く"
        >
          k0michi/cpsc ↗
        </a>
      </header>
      <button
        type="button"
        className={`menu-backdrop ${menuOpen ? "open" : ""}`}
        aria-label="メニューを閉じる"
        tabIndex={menuOpen ? 0 : -1}
        onClick={() => setMenuOpen(false)}
      />
      <div className="workspace">
        <aside
          id="snippet-navigation"
          className={`sidebar ${menuOpen ? "open" : ""}`}
        >
          <div className="sidebar-search">
            <input
              ref={searchRef}
              value={query}
              onChange={(event) => setQuery(event.target.value)}
              placeholder="Search"
              aria-label="スニペットを検索"
            />
            <kbd>/</kbd>
          </div>
          <nav aria-label="Snippet navigation">
            <NavLink to="/" end className={({isActive}) => `nav-item all-snippets ${isActive ? "active" : ""}`}>
              All snippets
            </NavLink>
            {visibleSnippets.map((snippet) => {
              const snippetNumber = snippets.indexOf(snippet) + 1;
              return (
              <NavLink key={snippet.slug} to={`/snippets/${snippet.slug}`} className={({isActive}) => `nav-item ${isActive ? "active" : ""}`}>
                <span
                  className="snippet-number"
                  style={categoryHueStyle(snippet.category, categories)}
                >
                  {String(snippetNumber).padStart(2, "0")}
                </span>
                {snippet.shortTitle}
              </NavLink>
              );
            })}
            {visibleSnippets.length === 0 && <span className="nav-empty">No snippets</span>}
          </nav>
        </aside>
        <main className="main-content">
          {children}
          <footer className="site-footer">
            <a
              href="https://creativecommons.org/publicdomain/zero/1.0/"
              target="_blank"
              rel="license noreferrer"
            >
              CC0 1.0 — Public Domain ↗
            </a>
          </footer>
        </main>
      </div>
    </div>
  );
}
