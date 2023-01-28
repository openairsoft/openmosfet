import terser from '@rollup/plugin-terser';
import del from 'rollup-plugin-delete';
import htmlBundle from 'rollup-plugin-html-bundle';
import { nodeResolve } from '@rollup/plugin-node-resolve';

export default [{
  input: './src/js/main.js',
  output: {
    file: 'build/bundle.js',
  },
  plugins: [
    del({
      targets: [
        'build/*',
        '!build/index.inline.html',
      ],
    }),
    nodeResolve({
      browser: true,
    }),
    terser(),
  ],
}, {
  input: './build/bundle.js',
  output: {
    file: 'build/bundle.js',
  },
  plugins: [
    htmlBundle({
      template: './build/index.inline.html',
      target: './build/index.rollup.html',
      inline: true,
      timestamp: false,
    }),
  ],
}];
