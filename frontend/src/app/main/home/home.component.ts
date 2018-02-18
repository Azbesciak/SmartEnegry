import {Component, OnInit} from '@angular/core';
import {DataService} from "../../functional/data/data.service";
import {from} from 'rxjs/observable/from';
import {groupBy, map, mergeMap, toArray} from 'rxjs/operators';
import "rxjs/add/operator/toArray"
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {ChartSeries, ChartValue, Consumption, Device, Granulation} from "../models/models";
import {MatSelectChange} from "@angular/material";

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {
  dateTo: Date;
  dateFrom: Date;
  devices: Array<Device>;
  consumptions: Consumption[] = [];
  chartDataBus = new BehaviorSubject<ChartSeries[]>([]);
  deviceId;

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


  // line, area
  autoScale = true;
  currentGranulation: Granulation;

  constructor(private data: DataService) {
  }

  ngOnInit(): void {
  this.currentGranulation = this.granulations[0];
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
    this.consumptions = consumptions;
    from(consumptions)
      .pipe(
        groupBy((c: Consumption) => c.device_id),
        mergeMap(g => g.toArray()),
        map(x => new ChartSeries(x, this.currentGranulation)),
        toArray()
      ).subscribe(x => this.chartDataBus.next(x))
  }

  canSearch() {
    return this.deviceId != null && this.deviceId != undefined
  }

  onGranulationChange($event: MatSelectChange) {
    this.groupConsumptions(this.consumptions)
  }
}
