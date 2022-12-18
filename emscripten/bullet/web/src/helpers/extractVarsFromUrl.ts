
const urlVarsRegexp = /[?&]+([^=&]+)=([^&]*)/gi;

export const extractVarsFromUrl = (): Record<string, string> => {
  const urlVars: Record<string, string> = {};

  let cap: RegExpExecArray | null = null;
  while (cap = urlVarsRegexp.exec(window.location.href)) {
    const key = cap[1];
    const value = cap[2];
    urlVars[key] = value;
  }
  // window.location.href.replace(urlVarsRegexp, (m, key: string, value) => {
  //   urlVars[key] = value;
  // });
  return urlVars;
};
