import { Component, OnInit } from '@angular/core';
import { JobsService } from '../jobs.service';
import { Script } from '../script';
import { ScriptService } from '../script.service';
import { Request } from '../job';
import { targets, Target} from '../target';

@Component({
  selector: 'app-script',
  templateUrl: './script.component.html',
  styleUrls: ['./script.component.css']
})
export class ScriptComponent implements OnInit {

  scripts: Script[] = [];
  selectedScript?: Script;
  content?: string;
  target = targets[0];

  targets = targets;

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
      serverUri: 'opc.tcp://' + this.target.host + ':' + this.target.port
    };
    this.jobService.createJob(req);
  }

  updateScript(script: Script, content: string): void
  {
    this.scriptService.updateScript(script, content);
  }

  onChange(e: Target): void {
    console.log(e);
    this.target = e;
  }


}
