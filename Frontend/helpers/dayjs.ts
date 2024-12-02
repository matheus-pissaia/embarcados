import _dayjs from 'dayjs'
import isSameOrAfter from 'dayjs/plugin/isSameOrAfter'

_dayjs.extend(isSameOrAfter)

export const dayjs = _dayjs
