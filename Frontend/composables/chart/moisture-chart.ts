import { Chart } from 'chart.js'
import type { OnChartHover } from './types'

type DataPoint = { x: string, y: number };
type MoistureChart = Chart<'line', DataPoint[], undefined>

export function updateMoistureChart(
    chart: MoistureChart,
    dataset: Ref<DataPoint[]>,
) {
    chart.data.datasets[0].data = dataset.value

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
                    fill: true, // Fill area under the line
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
                        unit: 'day',
                        displayFormats: {
                            day: 'DD/MM',
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
