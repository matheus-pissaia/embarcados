import { debounce } from 'throttle-debounce'

export class Debouncer {
    private static debouncers = new Map<string, debounce<() => void>>()

    public static execute(
        key: string,
        interval: number,
        callback: () => void,
    ) {
        let debouncer = this.debouncers.get(key)

        if (!debouncer) {
            debouncer = debounce(interval, () => callback())

            this.debouncers.set(key, debouncer)
        }

        debouncer()
    }

    public static destroy(key: string) {
        this.debouncers.get(key)?.cancel()

        this.debouncers.delete(key)
    }
}
