<template>
    <div class="h-screen place-content-center bg-gray-800 text-white">
        <div class="mx-auto grid grid-cols-2 w-fit gap-6">
            <div class="flex flex-col justify-between rounded-2xl bg-gray-700 p-4">
                <IconsWaterDrop />

                <div>
                    <p>Humidity</p>
                    <p class="text-2xl font-bold">
                        {{ percentage }}%
                    </p>
                </div>
            </div>

            <div class="flex size-40 flex-col justify-between rounded-2xl bg-blue-500 p-4">
                <IconsWave />

                <div>
                    <p>Water usage</p>
                    <p class="text-2xl font-bold">
                        21.5L
                    </p>
                </div>
            </div>

            <div class="col-span-2 rounded-2xl bg-gray-700 p-4">
                <div class="flex justify-between items-center">
                    <span>
                        Activation level:
                        <span class="font-bold">{{ activationLevel ?? '-' }}%</span>
                    </span>

                    <button
                        class="inline-flex w-fit items-center gap-2 rounded-lg bg-blue-500 p-2 font-bold"
                        @click="handleAction"
                    >
                        <span>{{ isEditing ? 'Save' : 'Edit' }}</span>

                        <IconsEdit />
                    </button>
                </div>

                <input
                    type="range"
                    class="mt-4 w-full"
                    :class="isEditing ? 'block' : 'hidden'"
                    value="40"
                    @input="handleInput"
                >
            </div>
        </div>
    </div>
</template>

<script setup lang="ts">
    type Rega = {
        umidade: number,
        dataHora: string
    }

    const BASE_URL = 'https://api-regador.onrender.com'

    const isEditing = ref(false)
    const percentage = ref(0)
    const activationLevel = ref<number>()
    const inputElement = ref<HTMLInputElement>()

    function handleAction() {
        isEditing.value = !isEditing.value

        if (!isEditing.value)
            handleSave()
    }

    function handleSave() {
        if (!inputElement.value) return

        const rangeValue = inputElement.value.value

        $fetch(BASE_URL + '/activation-level', {
            method: 'POST',
            body: { level: rangeValue },
        })
    }

    function handleInput(event: Event) {
        if (!isEditing.value || !activationLevel.value)
            return

        activationLevel.value = (event.target as HTMLInputElement).valueAsNumber
    }

    async function updatePercentage() {
        const data = await $fetch<Rega[]>(BASE_URL + '/regas')

        percentage.value = data.length ? data[data.length - 1].umidade : 0
    }

    async function updateActivationLevel() {
        const response = await $fetch<string>(BASE_URL + '/activation-level').then(Number)

        activationLevel.value = isNaN(response) ? 0 : response
    }

    onMounted(() => {
        updateActivationLevel()
        // TODO use setTimeout instead to avoid racing conditions
        setInterval(updatePercentage, 2_000)
    })
</script>
