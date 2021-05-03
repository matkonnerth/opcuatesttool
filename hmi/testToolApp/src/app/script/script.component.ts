import { Component, OnInit } from '@angular/core';
import { JobsService } from '../jobs.service';
import { Script } from '../script';
import { ScriptService } from '../script.service';
import { Request } from '../job';
import {NgForm} from '@angular/forms';

@Component({
  selector: 'app-script',
  templateUrl: './script.component.html',
  styleUrls: ['./script.component.css']
})
export class ScriptComponent implements OnInit {

  scripts: Script[] = [];
  selectedScript?: Script;
  content?: string;

  constructor(private scriptService: ScriptService, private jobService: JobsService) { }

  ngOnInit(): void {
    this.getScripts();
  }

  onSelect(script: Script): void {
    this.selectedScript = script;
    this.scriptService.getScriptContent(script).subscribe(c => this.content = c);
  }

  getScripts(): void {
    this.scriptService.getScripts()
      .subscribe(s => this.scripts = s);
  }

  newJob(script: Script): void {

    const req: Request = {
      name: 'myJob',
      script: script.name + '.chai',
      serverUri: 'opc.tcp://10.11.65.192:4840'
    };
    this.jobService.createJob(req);
  }

  updateScript(script: Script, content: string): void
  {
    this.scriptService.updateScript(script, content);
  }

  onSubmit(f: NgForm) {
    console.log(f.value);  // { first: '', last: '' }
  }
}