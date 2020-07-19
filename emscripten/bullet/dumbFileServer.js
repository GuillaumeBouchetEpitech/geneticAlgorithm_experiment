
const http  = require('http');
const url   = require('url');
const fs    = require('fs');
const path  = require('path');
const os    = require('os');
const port  = parseInt(process.argv[2] || 9000, 10);

// useful to test on local WiFi with a smartphone
const listWiFiIpAddresses = () => {

    const allInterfaces = [];

    const ifaces = os.networkInterfaces();

    Object.keys(ifaces).forEach((ifname) => {

        let alias = 0;

        ifaces[ifname].forEach((iface) => {

            // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
            if (iface.family.toLowerCase() !== 'ipv4' || iface.internal !== false)
                return;

            if (alias >= 1) {
                // this single interface has multiple ipv4 addresses
                allInterfaces.push(`ifname:${alias} ${iface.address}`);
            }
            else {
                // this interface has only one ipv4 adress
                allInterfaces.push(`ifname: ${iface.address}`);
            }

            ++alias;
        });
    });

    const uniqueValues = Array.from(new Set(allInterfaces));
    for (const value of uniqueValues)
        console.log(` => ${value}`);
}

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

const onFileRequest = (req, res) => {

    console.log(`${req.method} ${req.url}`);

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
    let contentType = "text/plain";

    // if it is a directory: list it's content
    if (fs.statSync(pathname).isDirectory()) {

        const content = fs.readdirSync(pathname, { withFileTypes: true });

        const allEntries = [];

        const splittedPath = parsedUrl.pathname.split("/").filter(item => item.length > 0);

        // console.log(req.headers.host, req.url)

        if (splittedPath.length > 0) {
            const copySplittedPath = splittedPath.slice(0);
            copySplittedPath.pop();
            const url = `http://${req.headers.host}/${copySplittedPath.join("/")}`
            allEntries.push(`<a href="${url}">${url}</a>`)
        }

        for (const dirent of content) {

            let url;
            if (splittedPath.length > 0)
                url = `http://${req.headers.host}/${splittedPath.join("/")}/${dirent.name}`;
            else
                url = `http://${req.headers.host}/${dirent.name}`;

            allEntries.push(`<a href="${url}">${url}</a>`)
        }

        data = allEntries.join("<br>");
        contentType = "text/html"; // <= override the content type
    }
    else {
        // read file from file system
        data = fs.readFileSync(pathname);
    }

    // if the file is found, set Content-type and send data
    res.setHeader('Content-type', formatsMap.get(ext) || contentType );
    res.setHeader('Last-Modified', (new Date()).toString());
    res.end(data);
}

const onListen = () =>  {
    console.log(`Server listening at http://127.0.0.1:${port}/`);
    console.log(` => http://127.0.0.1:${port}/dist/index.html`);

    listWiFiIpAddresses();
}

http.createServer(onFileRequest).listen(port, onListen);

