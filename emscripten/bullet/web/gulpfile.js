const gulp = require('gulp');
const browserify = require('browserify');
const source = require('vinyl-source-stream');
const tsify = require('tsify');
const uglify = require('gulp-uglify');
const sourcemaps = require('gulp-sourcemaps');
const buffer = require('vinyl-buffer');

const fs = require('fs');

const build = async (isDebug) => {

    const stream = browserify({
            basedir: '.',
            debug: (isDebug === true),
            entries: [
                './src/main.ts'
            ],
            cache: {},
            packageCache: {}
        })
        .plugin(tsify)
        .bundle()
        .pipe(source('bundle.js'))
        .pipe(buffer());

    if (isDebug === true) {
        stream.pipe(sourcemaps.init({loadMaps: true}))
              .pipe(sourcemaps.write('./'));
    }
    else {
        stream.pipe(uglify());
    }

    stream.pipe(gulp.dest('js'));

    return stream;
};

const copyToDestination = (done) => {
    fs.mkdirSync('../dist/js', { recursive: true });
    fs.copyFileSync('./pages/index.html', '../dist/index.html');
    fs.copyFileSync('./js/bundle.js', '../dist/js/bundle.js');
    done();
};

gulp.task('build-release', () => build(false));
gulp.task('build-debug', () => build(true));

gulp.task('copy-built-files', copyToDestination);

gulp.task('default', gulp.series('build-release', 'copy-built-files'));
gulp.task('debug', gulp.series('build-debug', 'copy-built-files'));
