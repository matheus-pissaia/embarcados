<template>
    <div class="p-16 rounded-16 bg-gray-700">
        <div v-if="loading" class="w-full h-[393px] grid place-content-center">
            <IconsLoading class="animate-spin" />
        </div>

        <template v-else>
            <div class="flex items-center gap-16">
                <div class="p-8 rounded-8 bg-blue-700">
                    <IconsWaterDrop />
                </div>

                <div>
                    <p class="text-gray-300 text-14">
                        Humidity
                    </p>
                    <p class="relative text-28 font-600 leading-1">
                        {{ percentage }}%
                    </p>
                </div>

                <PeriodSelector v-model="selectedPeriod" class="ml-auto" />
            </div>

            <canvas ref="canvasElement" class="max-h-296 aspect-video mt-16" />
        </template>
    </div>
</template>

<script setup lang="ts">
    import {
        setupChart,
        buildMoistureChart,
        updateMoistureChart,
        type DataPoint,
    } from '~/composables/chart'
    import { dayjs } from '~/helpers/dayjs'
    import { API } from '~/services/api'

    type TimePeriod = 1 | 7 | 30 // Days

    const loading = ref(true)
    const percentage = ref()
    const chartDataset = ref<DataPoint[]>()
    const selectedPeriod = ref<TimePeriod>(1)
    const canvasElement = ref<HTMLCanvasElement>()

    const filteredChartDataset = computed(() => {
        if (!chartDataset.value)
            return []

        return filterDatasetByDate(chartDataset.value, selectedPeriod.value)
    })

    function filterDatasetByDate(dataset: DataPoint[], period: TimePeriod): DataPoint[] {
        const timePeriodDate = dayjs().subtract(period, 'days').startOf('day')

        return dataset.filter(
            dataPoint => dayjs(dataPoint.x).isSameOrAfter(timePeriodDate),
        )
    }

    async function updateMoisturePercentage() {
        const data = await API.getMoisture()

        chartDataset.value = data.map(entry => ({ x: entry.dataHora, y: entry.umidade }))

        percentage.value = data.length ? data[data.length - 1].umidade : 0

        setTimeout(updateMoisturePercentage, 2_000)
    }

    setupChart(
        canvasElement,
        filteredChartDataset,
        buildMoistureChart,
        updateMoistureChart,
    )

    onMounted(updateMoisturePercentage)

    watch(percentage, () => loading.value = false, { once: true })
</script>

