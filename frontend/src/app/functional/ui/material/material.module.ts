import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';
import {BrowserAnimationsModule} from "@angular/platform-browser/animations";
import {
  MatToolbarModule, MatButtonModule, MatCardModule, MatFormFieldModule, MatInputModule,
  MatIconModule, MatDialogModule,
  MatButtonToggleModule, MatTooltipModule, MatOptionModule, MatSelectModule, MatListModule, MatSlideToggleModule,
  MatRadioModule, MatProgressSpinnerModule,
} from "@angular/material";
import {ReactiveFormsModule} from "@angular/forms";
import {Md2DatepickerModule, MdNativeDateModule} from "md2";

const MATERIAL_MODULES = [
  BrowserAnimationsModule,
  MatInputModule,
  MatFormFieldModule,
  MatCardModule,
  MatButtonModule,
  MatButtonToggleModule,
  MatToolbarModule,
  MatIconModule,
  MatDialogModule,
  MatOptionModule,
  MatSelectModule,
  ReactiveFormsModule,
  MatTooltipModule,
  Md2DatepickerModule,
  MdNativeDateModule,
  MatListModule,
  MatSlideToggleModule,
  MatRadioModule,
  MatProgressSpinnerModule
];

@NgModule({
  imports: [
    CommonModule,
    MATERIAL_MODULES
  ],
  exports: [
    BrowserAnimationsModule,
    MATERIAL_MODULES
  ],
  declarations: []
})
export class MaterialModule { }
