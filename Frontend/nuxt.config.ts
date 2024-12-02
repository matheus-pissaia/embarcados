// https://nuxt.com/docs/api/configuration/nuxt-config
export default defineNuxtConfig({
    runtimeConfig: {
        public: {
            baseApiUrl: process.env.BASE_API_URL,
        },
    },

    modules: ['@nuxtjs/tailwindcss'],

    devtools: { enabled: true },

    compatibilityDate: '2024-11-01',
})
