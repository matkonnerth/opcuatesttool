import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { JobsComponent } from './jobs/jobs.component'
import { ScriptComponent} from './script/script.component'

const routes: Routes = [
  { path: 'jobs', component: JobsComponent },
  { path: 'scripts', component: ScriptComponent}
];

@NgModule({
  imports: [RouterModule.forRoot(routes)],
  exports: [RouterModule]
})
export class AppRoutingModule { }
