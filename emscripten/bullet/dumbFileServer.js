
const http  = require('http');
const url   = require('url');
const fs    = require('fs');
const path  = require('path');
const port  = parseInt(process.argv[2] || 9000, 10);

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

    // if is a directory search for index file matching the extention
    if (fs.statSync(pathname).isDirectory())
        pathname += '/index' + ext;

    // read file from file system
    const data = fs.readFileSync(pathname);

    // if the file is found, set Content-type and send data
    res.setHeader('Content-type', formatsMap.get(ext) || 'text/plain' );
    res.end(data);
}

const onListen = () =>  {
    console.log(`Server listening at http://127.0.0.1:${port}/`);
    console.log(` => http://127.0.0.1:${port}/dist/index.html`);
}

http.createServer(onFileRequest).listen(port, onListen);

