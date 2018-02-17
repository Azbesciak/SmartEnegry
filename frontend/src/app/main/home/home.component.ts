import {Component, OnInit} from '@angular/core';
import {DataService} from "../../functional/data/data.service";
import {from} from 'rxjs/observable/from';
import {groupBy, map, mergeMap, toArray} from 'rxjs/operators';
import "rxjs/add/operator/toArray"

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {
  dateTo: Date;
  dateFrom: Date;
  devices: Array<Device>;
  chartData: any[] = [];

  view: any[] = [700, 400];

  deviceId;
  // options
  showXAxis = true;
  showYAxis = true;
  gradient = false;
  showLegend = true;
  showXAxisLabel = true;
  xAxisLabel = 'Time';
  showYAxisLabel = true;
  yAxisLabel = 'Consumption';

  colorScheme = {
    domain: ['#5AA454', '#A10A28', '#C7B42C', '#AAAAAA']
  };

  // line, area
  autoScale = true;

  onSelect(event) {
    console.log(event);
  }

  constructor(private data: DataService) {
  }

  ngOnInit(): void {
    this.data.getDevices()
      .then((devs: Array<Device>) => this.devices = devs);
    this.data.getConsumptions().then(c => this.groupConsumptions(c))
  }


  getConsumptions() {
    this.data.getConsumptionsBetween(
      this.deviceId,
      this.dateFrom ? this.dateFrom.getTime() : 0,
      this.dateTo ? this.dateTo.getTime() : new Date().getTime()
    ).then(c => this.groupConsumptions(c))
  }


  private groupConsumptions(consumptions) {
    const vals = consumptions.map(s => {
      return {name: new Date(s.measure_time), value: s.value, device_id: s.device_id}
    });
    from(vals)
      .pipe(
        groupBy((m: any) => m.device_id),
        mergeMap(g => g.toArray()),
        map(x => {
          return {
            name: x[0].device_id,
            series: x
          }
        }),
        toArray()
      ).subscribe(x => this.chartData = x)
  }

  canSearch() {
    return this.deviceId != null && this.deviceId != undefined
  }
}

interface Device {
  id: number
  description?: String
  state: boolean
}

interface Consumption {
  id: number
  measure_time: Date
  value: number
  device_id: number
}
