<template>
    <div class="col-span-2 rounded-16 bg-gray-700 p-16">
        <div class="flex justify-between items-center">
            <span>
                Activation level:
                <span class="font-600">{{ activationLevel ?? '-' }}%</span>
            </span>

            <button
                class="inline-flex w-fit items-center gap-8 rounded-8 bg-blue-700 p-8 font-600"
                @click="handleAction"
            >
                <span>{{ isEditing ? 'Save' : 'Edit' }}</span>

                <IconsEdit />
            </button>
        </div>

        <input
            type="range"
            class="mt-16 w-full"
            :class="isEditing ? 'block' : 'hidden'"
            value="40"
            @input="handleInput"
        >
    </div>
</template>

<script setup lang="ts">
    import { API } from '~/services/api'

    const isEditing = ref(false)
    const activationLevel = ref<number>()
    const inputElement = ref<HTMLInputElement>()

    function handleAction() {
        isEditing.value = !isEditing.value

        if (isEditing.value || !inputElement.value)
            return

        const rangeValue = inputElement.value.value

        API.setActivationLevel(rangeValue)
    }

    function handleInput(event: Event) {
        if (isEditing.value && activationLevel.value)
            activationLevel.value = (event.target as HTMLInputElement).valueAsNumber
    }

    async function updateActivationLevel() {
        const response = await API.getActivationLevel()

        activationLevel.value = isNaN(response) ? 0 : response
    }

    onMounted(() => updateActivationLevel)
</script>
