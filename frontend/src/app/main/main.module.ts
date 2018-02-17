import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import { HomeComponent } from './home/home.component';
import {UiModule} from "../functional/ui/ui.module";
import {FunctionalModule} from "../functional/functional.module";
import {DndModule} from "ng2-dnd";
import {NgxChartsModule} from "@swimlane/ngx-charts";

@NgModule({
  imports: [
    CommonModule,
    FunctionalModule,
    DndModule.forRoot(),
    NgxChartsModule
  ],
  exports: [HomeComponent],
  declarations: [
    HomeComponent
  ]
})
export class MainModule { }
