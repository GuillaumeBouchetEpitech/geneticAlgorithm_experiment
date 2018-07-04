
# nodejs node_modules/browserify/bin/cmd.js ./src/main.js -o ./dist/bundle.js

# wrong result???
# nodejs node_modules/browserify/bin/cmd.js ./src/main.js | nodejs node_modules/uglifyjs/bin/uglifyjs -c > ./dist/bundle.js
# /wrong result???

# nodejs node_modules/browserify/bin/cmd.js -g ./node_modules/uglifyify/index.js ./src/main.js > ./dist/bundle.js

# nodejs node_modules/browserify/bin/cmd.js --help advanced

# nodejs node_modules/browserify/bin/cmd.js -t ./node_modules/uglifyify/index.js ./src/main.js > ./dist/bundle.js

# nodejs node_modules/browserify/bin/cmd.js -t [ ./node_modules/uglifyify/index.js --ignore '**/node_modules/gamma/*' ] ./src/main.js 	> ./dist/bundle.js




# # no uglification
# nodejs node_modules/browserify/bin/cmd.js ./app/main.js -o ./dist/bundle.js

# # local uglification
# nodejs node_modules/browserify/bin/cmd.js -t ./node_modules/uglifyify/index.js ./webgl_experiment/app.js > ./webgl_experiment/dist/bundle.js

# global uglification
nodejs node_modules/browserify/bin/cmd.js -g ./node_modules/uglifyify/index.js ./app/main.js > ./dist/bundle.js
