import {Component, Input, OnInit} from '@angular/core';
import {Subject} from "rxjs/Subject";
import {ChartSeries} from "../models/models";

@Component({
  selector: 'app-chart',
  templateUrl: './chart.component.html',
  styleUrls: ['./chart.component.scss']
})
export class ChartComponent implements OnInit {

  colorScheme = {
    domain: ['#5AA454', '#A10A28', '#C7B42C', '#AAAAAA']
  };
  view: any[] = [700, 400];
  // options
  showXAxis = true;
  showYAxis = true;
  gradient = false;
  showLegend = true;
  showXAxisLabel = true;
  xAxisLabel = 'Time';
  showYAxisLabel = true;
  yAxisLabel = 'Consumption';
// line, area
  autoScale = true;
  chartData: ChartSeries[] = [];

  @Input()
  chartDataBus: Subject<ChartSeries[]>;
  constructor() {
  }

  ngOnInit() {
    this.chartDataBus.subscribe(x => this.chartData = x)
  }

}
