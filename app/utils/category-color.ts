export function hueFromCategory(category: string, categories: string[]) {
  const index = categories.indexOf(category);
  if (index < 0 || categories.length === 0) return 0;
  return (index * 360) / categories.length;
}

export function categoryHueStyle(category: string, categories: string[]) {
  return {
    "--snippet-hue": hueFromCategory(category, categories),
  } as React.CSSProperties;
}
