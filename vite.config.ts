import { defineConfig, loadEnv } from 'vite';
import laravel from 'laravel-vite-plugin';
import path from "node:path";
import importMetaUrlPlugin from '@codingame/esbuild-import-meta-url-plugin';

export default defineConfig(({ mode }) =>
{
    /**
     * all of these hoops need to be jumped through
     * in order to run laravel sail + vite behind
     * a reverse proxy.
     */
    const env = loadEnv(mode, process.cwd(), '');
    const appURL = new URL(env.APP_URL || 'http://localhost');
    
    return {
        plugins: [
            laravel({
                input: [
                    'resources/css/app/goldenlayout-base.scss',
                    'resources/css/app/goldenlayout-dark-theme.scss',
                    'resources/css/app/goldenlayout-light-theme.scss',
                    'resources/css/normalize.scss',
                    'resources/css/disagreed.scss',
                    'resources/js/disagreed.js',
                    'resources/css/markdown.scss',
                    'resources/css/app.scss',
                    'resources/js/app-preload.ts'
                ],
                refresh: true,
            }),
        ],
        optimizeDeps: {
            esbuildOptions: {
                plugins: [
                    importMetaUrlPlugin
                ]
            }
        },
        define: {
            rootDirectory: JSON.stringify(__dirname)
        },
        worker: {
            format: "es"
        },
        server: {
            host: '0.0.0.0',
            port: 5173,
            hmr: {
                host: appURL.hostname,
                protocol: (appURL.protocol === 'https:') ? 'wss' : 'ws',
                clientPort: parseInt(appURL.port || ((appURL.protocol === 'https:') ? "443" : "80")),
            },
            watch: {
                ignored: [
                    path.join(process.cwd(), "storage", "app", "workspaces", "**")
                ]
            }
        },
        
        /**
         * If we're in dev mode, we base on /vite/ otherwise use the default for building.
         */
        base: (mode !== "production") ? "/vite/" : "./",
    };
});
