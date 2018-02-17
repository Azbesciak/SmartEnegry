import {Component, OnInit} from '@angular/core';
import {DataService} from "../../functional/data/data.service";

@Component({
  selector: 'app-home',
  templateUrl: './home.component.html',
  styleUrls: ['./home.component.scss']
})
export class HomeComponent implements OnInit {
  dateTo: Date;
  dateFrom: Date;
  devices: Array<Device>;


  constructor(private data: DataService) {
  }

  ngOnInit(): void {
    this.data.getDevices()
      .then((devs: Array<Device>) => this.devices = devs)
      .then(() => console.log(this.devices))
  }

  getConsumptions() {

    this.data.getConsumptions().then(s => console.log(s))
  }
}

interface Device {
  id: number,
  description?: String,
  state: boolean
}
