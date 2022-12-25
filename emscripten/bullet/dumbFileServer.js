
const http  = require('http');
const url   = require('url');
const fs    = require('fs');
const path  = require('path');
const os    = require('os');
const zlib  = require('zlib');
const g_address  = process.argv[2] || '127.0.0.1';
const g_port  = parseInt(process.argv[3] || 9000, 10);

const worker_port = g_port;
const thread_port = g_port + 1;

const async_sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

// maps file extention to MIME typere
const formatsMap = new Map([
    [ '.ico',   'image/x-icon'          ],
    [ '.html',  'text/html'             ],
    [ '.js',    'text/javascript'       ],
    [ '.json',  'application/json'      ],
    [ '.css',   'text/css'              ],
    [ '.png',   'image/png'             ],
    [ '.jpg',   'image/jpeg'            ],
    [ '.wav',   'audio/wav'             ],
    [ '.mp3',   'audio/mpeg'            ],
    [ '.wav',   'audio/wav'             ],
    [ '.svg',   'image/svg+xml'         ],
    [ '.pdf',   'application/pdf'       ],
    [ '.doc',   'application/msword'    ],
    [ '.wasm',  'application/wasm'      ],
]);

const build_folder_page = (req, parsedUrl, pathname) => {

  const content = fs.readdirSync(pathname, { withFileTypes: true });

  const allEntries = [];

  const splittedPath = parsedUrl.pathname.split("/").filter(item => item.length > 0);

  const isRoot = (splittedPath.length < 1);
  const currFolder = isRoot ? '' : splittedPath.join("/");

  console.log('isRoot', isRoot);
  console.log('  currFolder    ', currFolder);

  // link to parent folder (if any)
  if (!isRoot) {
    const previousFolder = splittedPath.slice(0, splittedPath.length - 1).join("/");
    const url = `http://${req.headers.host}/${previousFolder}`;
    allEntries.push(`<a href="${url}"><b><= GO BACK</b></a>`);
  }

  const getUrl = (name) => {
    return !isRoot ? `${currFolder}/${name}` : name;
  }

  const { folders, files } = content.reduce((acc, dirent) => {
    if (dirent.isDirectory())
      acc.folders.push({ url: getUrl(dirent.name), name: dirent.name });
    else
      acc.files.push({ url: getUrl(dirent.name), name: dirent.name });
    return acc;
  }, { folders: [], files: [] });

  const allFolders = [];
  for (const data of folders) {
    allFolders.push(`<a href="http://${req.headers.host}/${data.url}"><b>${data.name}/</b></a>`);
  }
  const allFiles = [];
  for (const data of files) {
    allFiles.push(`<a href="http://${req.headers.host}/${data.url}">${data.name}</a>`);
  }

  return `
<!doctype html>
<html lang="en-us">
  <head>

    <meta charset="utf-8">
    <meta http-equiv="Content-Type" content="text/html; charset=utf-8">

    <title>TEST</title>

    <style>
      body, a, li {
        background-color: #0F0F0F;
        border: 0px 0px 0px 0px;
        margin: 0px 0px 0px 0px;
        padding: 0px 0px 0px 0px;
        font-family: arial;
        color: #8F8F8F;
      }

      a:link { text-decoration: none; }

      p, a {
        margin-left: 10px;
      }

      ul {
        line-height: 12px;
      }
    </style>

  </head>

  <body>

    <p>=> &#60;root&#62;/${currFolder}</p>

    ${allEntries.map(line => `    <b>${line}</b>`).join("<br>")}

    <p>FOLDERS (${allFolders.length})</p>
    <ul>
      ${allFolders.map(line => `      <li>${line}</li>`).join("<br>")}
    </ul>
    <p>FILES (${allFiles.length})</p>
    <ul>
      ${allFiles.map(line => `      <li>${line}</li>`).join("<br>")}
    </ul>
  </body>
</html>
`;
}

const on_file_request = async (threading_headers, req, res) => {

    // parse URL
    const parsedUrl = url.parse(req.url);
    // extract URL path
    let pathname = `.${parsedUrl.pathname}`;
    // based on the URL path, extract the file extention. e.g. .js, .doc, ...
    const ext = path.parse(pathname).ext;

    if (!fs.existsSync(pathname)) {
        // if the file is not found, return 404
        res.statusCode = 404;
        res.end(`File "${pathname}" not found!`);
        return;
    }

    let data;

    // if it is a directory: list it's content
    const fileStats = fs.statSync(pathname);
    if (fileStats.isDirectory()) {
      data = build_folder_page(req, parsedUrl, pathname);
      res.setHeader('Content-type', "text/html");
      res.setHeader('Last-Modified', (new Date()).toString());
    }
    else {
      // read file from file system
      data = fs.readFileSync(pathname);
      res.setHeader('Content-type', formatsMap.get(ext) || "text/plain");
      res.setHeader('Last-Modified', fileStats.mtime.toString());
    }

    const originalSize = data.length;
    let sizeToSend = originalSize;

    if (
        req.headers['accept-encoding'] &&
        req.headers['accept-encoding'].indexOf("gzip") >= 0
    ) {
        res.setHeader('Content-Encoding', "gzip" );
        data = zlib.gzipSync(data);
    }

    sizeToSend = data.length;

    // // attempt at preventing browser caching (for debug)
    // res.setHeader('Last-Modified', (new Date()).toString());


    if (threading_headers === true) {
        res.setHeader('Cross-Origin-Opener-Policy', "same-origin");
        res.setHeader('Cross-Origin-Embedder-Policy', "require-corp");
    }

    // await async_sleep(500);

    let logMsg = `${req.method} ${req.url} ${sizeToSend / 1000}Kb`;
    if (sizeToSend < originalSize)
        logMsg += ` (${originalSize / 1000}Kb, ${(originalSize / sizeToSend).toFixed(1)}x)`;
    console.log(logMsg);

    res.end(data);
}

// useful to test on local WiFi with a smartphone
const list_WiFi_IpAddresses = (inPort) => {

    const allInterfaces = [];

    const ifaces = os.networkInterfaces();

    Object.keys(ifaces).forEach((ifname) => {

        let alias = 0;

        ifaces[ifname].forEach((iface) => {

            // skip over internal (i.e. 127.0.0.1)
            if (iface.internal !== false)
                return;

            // skip over non-ipv4 addresses
            if (iface.family.toLowerCase() !== 'ipv4')
                return;

            if (alias >= 1) {
                // this single interface has multiple ipv4 addresses
                allInterfaces.push(`ifname:${alias} http://${iface.address}:${inPort}/`);
            }
            else {
                // this interface has only one ipv4 adress
                allInterfaces.push(`ifname: http://${iface.address}:${inPort}/`);
            }

            ++alias;
        });
    });

    const uniqueInterfaces = Array.from(new Set(allInterfaces));
    for (const interface of uniqueInterfaces)
        console.log(` => ${interface}`);
}

const make_server = (inPort, threading_headers) => {
    http.createServer((req, res) => on_file_request(threading_headers, req, res))
        .listen(inPort, g_address, () => {
            console.log(`Server listening`);
            console.log(`=> http://127.0.0.1:${inPort}/`);
            list_WiFi_IpAddresses(inPort);
        });
};

make_server(worker_port, false);
make_server(thread_port, true);

