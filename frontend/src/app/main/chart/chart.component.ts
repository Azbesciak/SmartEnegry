import {Component, Input, OnInit, ViewEncapsulation} from '@angular/core';
import {Subject} from "rxjs/Subject";
import {ChartSeries, ChartValue, Consumption, Granulation} from "../models/models";
import {groupBy, map, mergeMap, toArray} from "rxjs/operators";
import {from} from "rxjs/observable/from";
import {MatSelectChange} from "@angular/material";

@Component({
  selector: 'app-chart',
  templateUrl: './chart.component.html',
  styleUrls: ['./chart.component.scss'],
  encapsulation: ViewEncapsulation.None
})
export class ChartComponent implements OnInit {

  showXAxis = true;
  showYAxis = true;
  gradient = true;
  showLegend = true;
  showXAxisLabel = true;
  xAxisLabel = 'Time';
  showYAxisLabel = true;
  yAxisLabel = 'Consumption';
// line, area
  autoScale = true;
  chartData: ChartSeries[] = [];
  consumptions: Consumption[];

  @Input()
  consumptionsBus: Subject<Consumption[]>;
  currentGranulation: Granulation;

  granulations = [
    new Granulation("Hour", val => ChartValue.fromTime(val.value, val.name.setMinutes(0, 0, 0))),
    new Granulation("Day", val => ChartValue.fromTime(val.value, val.name.setHours(0, 0, 0, 0))),
    new Granulation("Week", val => {
      const temp = new Date(val.name);
      temp.setHours(0, 0, 0, 0);
      temp.setDate(temp.getDate() - temp.getDay() + 1);
      return new ChartValue(val.value, temp);
    })
  ];
  constructor() {
  }

  onGranulationChange($event: MatSelectChange) {
    this.groupConsumptions(this.consumptions)
  }

   private groupConsumptions(consumptions) {
    this.consumptions = consumptions;
    from(consumptions)
      .pipe(
        groupBy((c: Consumption) => c.device_id),
        mergeMap(g => g.toArray()),
        map(x => new ChartSeries(x, this.currentGranulation)),
        toArray()
      ).subscribe(x => this.chartData = x)
  }

  ngOnInit() {
    this.currentGranulation = this.granulations[0];
    this.consumptionsBus.subscribe(x => this.groupConsumptions(x))
  }

}
