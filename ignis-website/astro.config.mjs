import { defineConfig } from 'astro/config';

export default defineConfig({
  site: 'https://ignis-tvc.tech',
  vite: {
    assetsInclude: ['**/*.glb', '**/*.gltf']
  }
});