import {
    CategoryScale,
    Chart,
    Filler,
    Legend,
    LinearScale,
    LineController,
    LineElement,
    PointElement,
    TimeScale,
    Tooltip,
    type ChartType,
} from 'chart.js'
import { nanoid } from 'nanoid'
import type { OnChartHover } from './types'
import { Debouncer } from '~/helpers/Debouncer'

Chart.register(
    Tooltip,
    LineController,
    LineElement,
    PointElement,
    LinearScale,
    CategoryScale,
    Filler,
    Legend,
    TimeScale,
    Tooltip,
)

/**
 * Initializes and manages the lifecycle of a Chart.js chart instance.
 * This function encapsulates the logic for creating a new chart or updating an existing one
 * based on changes in its dependencies, such as its canvas element or dataset.
 * It leverages Vue's reactivity and Chart.js's charting capabilities to provide a dynamic charting experience.
 *
 * ## Generics:
 * - `SelectedChartType`: Extends ChartType from Chart.js, indicating the type of chart to be created (e.g., 'line', 'doughnut').
 * - `RawDatasetType`: The raw type of the dataset provided for the chart. This can be any user-defined type that represents the data before it is processed for charting.
 * - `MappedDatasetType`: The processed type of the dataset after being formatted for use with the specified chart type in Chart.js.
 * - `LabelType`: The type used for the chart's labels, typically a string or number. (optional)
 *
 * ## Parameters:
 * - `canvas`: A Ref to an HTMLCanvasElement or undefined, representing the canvas element where the chart will be drawn.
 * - `dataset`: A Ref to the raw dataset of type `RawDatasetType` that the chart will display.
 * This dataset is reactive and can trigger chart updates when changed.
 * - `buildFunction`: A function that takes an HTMLCanvasElement and the reactive dataset, and returns an instance of a Chart.js chart.
 * This function is responsible for creating the chart instance with the appropriate configuration and data mapping.
 * - `updateFunction`: A function that updates the chart instance with new data.
 * It is called whenever the dataset changes, allowing for dynamic updates to the chart without full reinitialization.
 *
 * ## Lifecycle Management:
 * - The function sets up a Vue watch effect on the `canvas` and `dataset` refs. When either changes, it either creates a new chart (if none exists) or updates the existing chart with the new dataset, after a debounce period to avoid excessive updates.
 * - It also registers a hook with `onBeforeUnmount` to ensure that any debounced actions are canceled and resources are properly cleaned up when the component that owns the chart is destroyed.
 *
 * ## Usage:
 * - This function is designed to abstract away the boilerplate of creating and updating charts, managing lifecycle hooks, and integrating with Vue's reactivity system.
 */
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
    const chart = shallowRef<Chart<SelectedChartType, MappedDatasetType, LabelType>>()
    const key = `chart-${ nanoid() }`

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
