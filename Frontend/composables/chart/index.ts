import {
    CategoryScale,
    Chart,
    LinearScale,
    LineController,
    LineElement,
    PointElement,
    TimeScale,
    type ChartType,
} from 'chart.js'
import { nanoid } from 'nanoid'
import type { OnChartHover } from './types'
import { Debouncer } from '~/helpers/Debouncer'

Chart.register(
    LineController,
    LineElement,
    PointElement,
    LinearScale,
    CategoryScale,
    TimeScale,
)

export function setupChart<SelectedChartType extends ChartType, RawDatasetType, MappedDatasetType, LabelType>(
    canvas: Ref<HTMLCanvasElement | undefined>,
    dataset: Ref<RawDatasetType>,
    buildFunction: (
        canvas: HTMLCanvasElement,
        dataset: Ref<RawDatasetType>,
        onHover?: OnChartHover,
    ) => (Chart<SelectedChartType, MappedDatasetType, LabelType>),
    updateFunction: (
        chart: Chart<SelectedChartType, MappedDatasetType, LabelType>,
        dataset: Ref<RawDatasetType>,
    ) => void,
    onHover?: OnChartHover,
) {
    const key = `chart-${ nanoid() }`
    const chart = shallowRef<Chart<SelectedChartType, MappedDatasetType, LabelType>>()

    onBeforeUnmount(() => Debouncer.destroy(key))

    watch(
        [canvas, dataset],
        () => nextTick(() => {
            const canvasValue = canvas.value

            if (!canvasValue || !process.client)
                return

            Debouncer.execute(key, 250, () => {
                if (chart.value)
                    return updateFunction(chart.value, dataset)

                chart.value = buildFunction(canvasValue, dataset, onHover)
            })
        }),
        { immediate: true },
    )
}

export * from './moisture-chart'
