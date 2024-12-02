import { Chart } from 'chart.js'
import dayjs from 'dayjs'
import 'chartjs-adapter-dayjs-4/dist/chartjs-adapter-dayjs-4.esm'
import type { OnChartHover } from './types'

export type DataPoint = {
    x: string, // ISO date
    y: number // Moisture percentage
};
type MoistureChart = Chart<'line', DataPoint[], undefined>

function getTimeUnit(dataset: Ref<DataPoint[]>) {
    if (dataset.value.length < 2)
        return 'hour'

    const firstDate = dayjs(dataset.value[0].x)
    const lastDate = dayjs(dataset.value[dataset.value.length - 1].x)

    return lastDate.diff(firstDate, 'hour') > 24
        ? 'day'
        : 'hour'
}

export function updateMoistureChart(
    chart: MoistureChart,
    dataset: Ref<DataPoint[]>,
) {
    chart.data.datasets[0].data = dataset.value
    // @ts-ignore
    chart.options.scales!.x!.time!.unit = getTimeUnit(dataset)

    chart.update()
}

export function buildMoistureChart(
    canvas: HTMLCanvasElement,
    dataset: Ref<DataPoint[]>,
    onHover?: OnChartHover,
): MoistureChart {
    const context = canvas.getContext('2d')

    if (!context)
        throw new Error('Canvas context not found')

    return new Chart(context, {
        type: 'line',
        data: {
            datasets: [
                {
                    data: dataset.value,
                    backgroundColor: 'transparent',
                    borderColor: '#3b82f6',
                    borderWidth: 2,
                    pointRadius: 0, // No points
                    tension: 0.3,
                },
            ],
        },
        options: {
            responsive: true,
            maintainAspectRatio: false,
            scales: {
                x: {
                    type: 'time',
                    time: {
                        unit: getTimeUnit(dataset),
                        displayFormats: {
                            hour: 'h:mm',
                            day: 'MMM D',
                        },
                    },
                },
                y: {
                    max: 100,
                    beginAtZero: true,
                },
            },
            plugins: {
                legend: {
                    display: false,
                },
                tooltip: {
                    enabled: false,
                },
            },
            interaction: onHover
                ? { intersect: false }
                : undefined,
            onHover,
        },
    })
}
