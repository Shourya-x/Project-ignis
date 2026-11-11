// astro.config.mjs
import { defineConfig } from 'astro/config';

export default defineConfig({
  site: 'https://ignis-tvc.tech',
  outDir: './dist', // Ensure build output is in 'dist'
  publicDir: './public', // Ensure static assets are from 'public'
  vite: {
    assetsInclude: ['**/*.glb', '**/*.gltf'],
    // Pre-bundle Three.js so the dev server never serves a stale/outdated dep (blocked module)
    optimizeDeps: {
      include: ['three', 'three/examples/jsm/controls/OrbitControls.js'],
    },
    css: {
      // This ensures CSS is properly extracted and referenced
      devSourcemap: true,
    },
  },
});