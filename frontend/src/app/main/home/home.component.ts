import {Component, OnInit, ViewChild} from '@angular/core';
import {DataService} from "../../functional/data/data.service";
import "rxjs/add/operator/toArray"
import {BehaviorSubject} from "rxjs/BehaviorSubject";
import {Consumption, Device} from "../models/models";
import {MatRadioGroup, MatSelectionList} from "@angular/material";
import {Md2Datepicker} from "md2";

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
  maxDate = this.getEndOfTheDay();
  consumptionsBus = new BehaviorSubject<Consumption[]>([]);
  @ViewChild('devicesView')
  devicesView: MatSelectionList;

  @ViewChild('dateIntervalRadio')
  dateIntervalRadio: MatRadioGroup;

  @ViewChild("dateFromDatePick")
  dateFromDatePick: Md2Datepicker;

  @ViewChild("dateToDatePick")
  dateToDatePick: Md2Datepicker;

  selectedDateInterval = null;

  dateIntervals = [
    {name: "Today", fun: () => {
      this.dateFrom = new Date();
      this.dateFrom.setHours(0,0,0,0);
    }},
    {name: "Last week", fun: () => {
      this.dateFrom = new Date();
      this.dateFrom.setHours(0,0,0,0);
      this.dateFrom.setDate(this.dateFrom.getDate() - 7);
    }},
    {name: "Last month", fun: () => {
      this.dateFrom = new Date();
      this.dateFrom.setHours(0,0,0,0);
      this.dateFrom.setMonth(this.dateFrom.getMonth() - 1);
    }},
    {name: "All", fun: () => {this.dateFrom = null; this.dateTo = null}}
  ];
  isLoading: boolean;

  private getEndOfTheDay() {
    const date = new Date();
    date.setHours(23,59,59,999);
    return date;
  }

  constructor(private data: DataService) {
  }

  ngOnInit(): void {
    this.data.getDevices()
      .then((devs: Array<Device>) => this.devices = devs);
    this.fetchConsumptions(() => this.data.getConsumptions())
  }


  private fetchConsumptions(fun: () => Promise<any>) {
    this.isLoading = true;
    fun().then(c => this.consumptionsBus.next(c))
      .then(() => this.isLoading = false)
      .catch(e => this.isLoading = false)
  }

  getConsumptions() {
    let devicesIds = this.devicesView.selectedOptions.selected
      .map(x => x.value.id)
      .join(",");
    this.fetchConsumptions(() =>  this.data.getConsumptionsBetween(
      devicesIds,
      this.dateFrom ? this.dateFrom.getTime() : 0,
      this.dateTo ? this.dateTo.getTime() : this.getEndOfTheDay().getTime()
    ));
  }

  canSearch() {
    return this.devicesView.selectedOptions.selected.length > 0 && !this.isLoading
  }
  onStateChange(event, dev: Device) {
    this.data.changeDeviceState(dev.id, event.checked)
      .then((x: Device) => dev.state = x.state)
      .catch()

  }

  stopProp($event) {
    $event.stopPropagation()
  }

  onDateIntervalChange($event) {
    this.selectedDateInterval = $event.value;
    console.log(this.selectedDateInterval);
    this.dateTo = null;
    this.selectedDateInterval.fun();
  }

  onDateChange() {
    this.selectedDateInterval = null;
  }
}
