import type { ActiveElement, ChartEvent } from 'chart.js'

export type OnChartHover = (event: ChartEvent, chartElements: ActiveElement[]) => void
