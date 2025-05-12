import { defineConfig } from 'vitest/config';

export default defineConfig({
  test: {
    name: 'lynx-types',
    include: ['test/**/*.test-d.ts'],
  },
});
