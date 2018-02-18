import {Injectable} from '@angular/core';
import {HttpClient} from "@angular/common/http";
import {environment as env} from "../../../environments/environment"

@Injectable()
export class DataService {

  constructor(private http: HttpClient) {
  }

  getConsumptions() {
    return this.get("consumptions")
  }

  getConsumptionsBetween(devId: string, dateFrom: number, dateTo: number) {
    return this.get(`consumptions/${devId}/${dateFrom}/${dateTo}`)
  }

  getDevices() {
    return this.get("device")
  }

  getDevice(id) {
    return this.get(`device/${id}`)
  }

  changeDeviceState(id, isEnabled) {
    return this.post(`device/${id}/${isEnabled ? 1 : 0}`)
  }

  get(url) {
    return this.http.get(toApi(url)).toPromise()
  }
  post(url) {
    return this.http.post(toApi(url), {}).toPromise()
  }
}

function toApi(url: string): string {
  return `${env.apiRoot}/${url}/`
}
