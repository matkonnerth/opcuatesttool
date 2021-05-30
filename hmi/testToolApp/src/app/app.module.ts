import { NgModule } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpClientModule } from '@angular/common/http';
import { CodemirrorModule } from '@ctrl/ngx-codemirror';
import { NgTerminalModule } from 'ng-terminal';

import { AppRoutingModule } from './app-routing.module';
import { FormsModule }   from '@angular/forms';
import { AppComponent } from './app.component';
import { JobsComponent } from './jobs/jobs.component';
import { ScriptComponent } from './script/script.component';
import { ReplComponent } from './repl/repl.component';


@NgModule({
  declarations: [
    AppComponent,
    JobsComponent,
    ScriptComponent,
    ReplComponent
  ],
  imports: [
    BrowserModule,
    AppRoutingModule,
    HttpClientModule,
    FormsModule,
    CodemirrorModule,
    NgTerminalModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
