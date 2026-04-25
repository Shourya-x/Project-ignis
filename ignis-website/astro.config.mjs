// astro.config.mjs
import { defineConfig } from 'astro/config';

export default defineConfig({
  site: 'https://ignis-tvc.tech',
  outDir: './dist', // Ensure build output is in 'dist'
  publicDir: './public', // Ensure static assets are from 'public'
  vite: {
    assetsInclude: ['**/*.glb', '**/*.gltf'],
    css: {
      // This ensures CSS is properly extracted and referenced
      devSourcemap: true,
    },
  },
});