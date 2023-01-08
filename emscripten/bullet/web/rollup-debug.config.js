
import typescript from '@rollup/plugin-typescript';
import commonjs from '@rollup/plugin-commonjs';
import terser from '@rollup/plugin-terser';

export default {
  input: 'src/main.ts',
  output: {
    file: './js/bundle.js',
    format: 'cjs',
  },
  plugins: [
    typescript(),
    commonjs(),
    // terser(),
  ],
};
