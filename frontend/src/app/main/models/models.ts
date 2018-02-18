import {concatAll, groupBy, map, mergeMap, toArray} from "rxjs/operators";
import {from} from "rxjs/observable/from";

export interface Device {
  id: number
  description?: String
  state: boolean
}

export interface Consumption {
  id: number
  measure_time: Date
  value: number
  device_id: number
}

export class Granulation {
  constructor(public type: string, public modifier: (val: ChartValue) => ChartValue) {
  }
}

export class ChartSeries {
  name: string;
  series: ChartValue[] = [];

  constructor(values, granulation: Granulation) {
    this.name = values[0].device_id;
    from(values).pipe(
      map((c: Consumption) => ChartValue.fromConsumption(c)),
      map(cv => granulation.modifier(cv)),
      groupBy((cv: ChartValue) => cv.name.getTime()),
      mergeMap(cvs => cvs.toArray()),
      map(cvs => {
        const sumOfValues = cvs
          .map(cv => cv.value)
          .map(strVal => +strVal)
          .reduce((acc, cur) => acc + cur, 0);
        return new ChartValue(sumOfValues / cvs.length, cvs[0].name)
      }),
      toArray(),
      toArray(),
    ).subscribe(
      obj => obj.map(mappedSeries => this.series = mappedSeries)
    )
  }
}

export class ChartValue {
  constructor(public value: number, public name: Date) {
  }

  static fromConsumption(c: Consumption) {
    return new ChartValue(c.value, new Date(c.measure_time))
  }

  static fromTime(value: number, name: any) {
    return new ChartValue(value, new Date(name))
  }
}
