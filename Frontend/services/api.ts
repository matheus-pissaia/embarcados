export class API {
    public static getMoisture() {
        return $fetch<{ umidade: number, dataHora: string }[]>(this.url('/regas'))
    }

    public static getActivationLevel() {
        return $fetch(this.url('/activation-level')).then(Number)
    }

    public static setActivationLevel(level: string) {
        return $fetch(this.url('/activation-level'), {
            method: 'POST',
            body: { level },
        })
    }

    private static url(path: string) {
        return `${ this.baseUrl }${ path }`
    }

    private static get baseUrl() {
        return useRuntimeConfig().public.baseApiUrl
    }
}
