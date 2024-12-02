<template>
    <div class="rounded-16 bg-white/5 grid grid-cols-3 divide-x border border-white/10 overflow-clip">
        <button
            v-for="option in options"
            :key="option.id"
            :class="{ 'bg-blue-700': selectedOption === option.id }"
            class="font-600 px-8 py-4 text-14 transition-all duration-300 border-inherit"
            @click="selectedOption = option.id"
        >
            {{ option.label }}
        </button>
    </div>
</template>

<script setup lang="ts">
    const options = [
        { label: '1 day', id: 1 },
        { label: '7 days', id: 7 },
        { label: '30 days', id: 30 },
    ]

    const selectedOption = defineModel<string | number>('modelValue', { required: true })
</script>
